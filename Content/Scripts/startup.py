# Configures the Unreal Engine Python environment

import sys
import os

env_var = "PYTHON_LIB_PATH"
ext_lib_path = os.getenv(env_var)
if ext_lib_path is None:    
    raise Exception(
     f"Environment variable {env_var} not found! " + 
     "Make sure that you have created a PYTHON_LIB_PATH environment variable and pointed it to your Python environment's site-packages folder.")
if ext_lib_path not in sys.path:
    sys.path.append(ext_lib_path)