import pyspark
import pyspark.sql.functions
import json
import argparse
import sys
import os


def generate_bounds_conf(input_csv_path: str, output_json_path: str):
    """
    Generates a JSON file with the center and radius of the smallest circle which encompasses all
    points within the CSV data. Also includes the min/max values along each axis X, Y, and Z.
    :param input_csv_path: (str) Path to the CSV file.
    :param output_json_path: (str) Path to the JSON file to be generated.
    :return N/A
    """
    # Load spark
    spark = pyspark.sql.SparkSession.builder.getOrCreate()
    # Load a CSV into a Spark DataFrame.
    df = spark.read.csv(input_csv_path, header=True, inferSchema=True)
    # Assumed columns of CSV: ..., X, Y, Z, ...
    # Reduce to get min/max values along each axis
    minmax = df.agg(
        pyspark.sql.functions.min(df.X).alias("minX"),
        pyspark.sql.functions.max(df.X).alias("maxX"),
        pyspark.sql.functions.min(df.Y).alias("minY"),
        pyspark.sql.functions.max(df.Y).alias("maxY"),
        pyspark.sql.functions.min(df.Z).alias("minZ"),
        pyspark.sql.functions.max(df.Z).alias("maxZ")
    ).collect()[0]
    # Get the largest value for each of the X, Y, and Z dimensions
    largest_x = max(abs(minmax.minX), abs(minmax.maxX))
    largest_y = max(abs(minmax.minY), abs(minmax.maxY))
    largest_z = max(abs(minmax.minZ), abs(minmax.maxZ))
    # Obtain the center and radius of the smallest circle containing all points
    center = ((minmax.minX + minmax.maxX)/2, (minmax.minY + minmax.maxY)/2, (minmax.minZ + minmax.maxZ)/2)
    center_x, center_y, center_z = center
    radius = max(largest_x, largest_y, largest_z) / 2
    # Extract the file name from the path
    file_name = os.path.basename(input_csv_path).replace(".csv", "")
    # Save the center and radius to a JSON file, along with the min/max values along each axis
    with open(output_json_path, 'w+') as f:
        json.dump([{
            "NAME": file_name,
            "CENTER_X": center_x,
            "CENTER_Y": center_y,
            "CENTER_Z": center_z,
            "RADIUS": radius,
            "X_MIN": minmax.minX,
            "X_MAX": minmax.maxX,
            "Y_MIN": minmax.minY,
            "Y_MAX": minmax.maxY,
            "Z_MIN": minmax.minZ,
            "Z_MAX": minmax.maxZ
        }], f)
    # Note: it is necessary for the JSON to be an array of objects, even if there is only one object, as Unreal expects
    # an array of objects.


generate_bounds_conf("C:\\Users\\PC\\dev\\data-wielder\\unreal-data-visualization\\Content\\Data\\star_spatial_data_0_25.csv", "C:\\Users\\PC\\dev\\data-wielder\\unreal-data-visualization\\Content\\Data\\star_spatial_data_bounds_0_25.json")

# Enable running this script from the command line
if __name__ == "__main__":
    # Log errors and stdout to files
    script_name = os.path.splitext(os.path.basename(__file__))[0]
    parent_dir = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
    log_dir = os.path.join(parent_dir, "Logs")
    stdout_file = os.path.join(log_dir, script_name + "_stdout.txt")
    stderr_file = os.path.join(log_dir, script_name + "_stderr.txt")
    sys.stdout = open(stdout_file, 'w')
    sys.stderr = open(stderr_file, 'w')
    # Parse arguments
    parser = argparse.ArgumentParser()
    parser.add_argument("--input-csv-path", type=str, required=True, help="Path to the CSV file.")
    parser.add_argument("--output-json-path", type=str, required=True, help="Path to the JSON file to be generated.")
    args = parser.parse_args()
    # Run the script
    generate_bounds_conf(args.input_csv_path, args.output_json_path)