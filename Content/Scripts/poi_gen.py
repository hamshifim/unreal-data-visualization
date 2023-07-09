import pyspark
from pyspark.sql import Row
import pyspark.sql.functions
import json
import argparse
import sys
import os


def generate_poi_conf(input_csv_path: str, output_json_path: str):
    """
    Generates a JSON file with the min/max values of the X, Y, and Z columns of a CSV file.
    :param input_csv_path: (str) Path to the CSV file.
    :param output_json_path: (str) Path to the JSON file to be generated.
    :return N/A
    """
    # Load spark
    spark = pyspark.sql.SparkSession.builder.getOrCreate()
    # Load a CSV into a Spark DataFrame.
    df = spark.read.csv(input_csv_path, header=True, inferSchema=True)
    # Assumed columns of CSV: Index, X, Y, Z
    # Reduce to get min/max X, Y, and Z values
    minmax = df.agg(
        pyspark.sql.functions.min(df.X).alias('min_X'),
        pyspark.sql.functions.max(df.X).alias('max_X'),
        pyspark.sql.functions.min(df.Y).alias('min_Y'),
        pyspark.sql.functions.max(df.Y).alias('max_Y'),
        pyspark.sql.functions.min(df.Z).alias('min_Z'),
        pyspark.sql.functions.max(df.Z).alias('max_Z'),
    ).collect()[0]
    # Save as JSON
    # Convert the minmax object to a dict and save as JSON with Python's json library
    with open(output_json_path, 'w+') as f:
        json.dump(minmax.asDict(), f)


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
    generate_poi_conf(args.input_csv_path, args.output_json_path)