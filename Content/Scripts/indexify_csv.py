# PLACEHOLDER - Does not work yet.

from pyspark.sql import SparkSession
from pyspark.sql.functions import monotonically_increasing_id


def indexify_csv(input_file_path):
    # start a spark session
    spark = SparkSession.builder.appName('indexify').getOrCreate()
    # load the data
    df = spark.read.format('csv').option('header', 'true').load(input_file_path)
    # add index column
    df = df.withColumn('Index', monotonically_increasing_id())
    # only do with a small dataset
    df.coalesce(1)
    # write back to csv
    df.write.csv(input_file_path, header=True)

indexify_csv("C:\\Users\\PC\\dev\\data-wielder\\unreal-data-visualization\\Content\\Data\\cluster_info.csv")
