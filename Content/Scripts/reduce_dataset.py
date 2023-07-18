from pyspark.ml.clustering import KMeans
from pyspark.ml.evaluation import ClusteringEvaluator
from pyspark.sql import SparkSession
from pyspark.ml.feature import VectorAssembler


def reduce_dataset(spark, percent_to_reduce_to, input_file_path, output_file_path):
    # Load dataset
    dataset = spark.read.format("csv").option("header","true").load(input_file_path)

    # Convert columns to numeric
    for col in dataset.columns[1:]:
        dataset = dataset.withColumn(col, dataset[col].cast("float"))

    # Sample the dataset
    sampled_data = dataset.sample(False, percent_to_reduce_to)

    # Save the sampled dataset to a file
    sampled_data.repartition(1).write.format("csv").option("header", "true").save(output_file_path)


spark = SparkSession.builder.appName("KMeans").getOrCreate()
reduced_dataset = reduce_dataset(spark, 0.25, "C:\\Users\\PC\\dev\\data-wielder\\unreal-data-visualization\\Content\\Data\\star_spatial_data.csv", "C:\\Users\\PC\\dev\\data-wielder\\unreal-data-visualization\\Content\\Data\\star_spatial_data_0_25.csv")