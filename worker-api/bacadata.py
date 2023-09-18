from pymongo import MongoClient
from bson import ObjectId
import json

# Import the credentials and connection details from rahasia.py
from config import mongoConnString, dbName, collectionName

# Connect to the MongoDB server
client = MongoClient(mongoConnString)

# Access the database and collection
db = client[dbName]
collection = db[collectionName]

# Define sorting and limit options
options = {'sort': [('timestamp', -1)], 'limit': 100}

# Define the query filter
def get_data():
  # Perform the query
  cursor = collection.find({}, **options)

  # Convert ObjectId values to strings in the documents
  def jsonify_document(document):
      for key, value in document.items():
          if isinstance(value, ObjectId):
              document[key] = str(value)
      return document

  # Collect data in a list, converting ObjectId values
  dataArray = [jsonify_document(document) for document in cursor]

  # Print JSON data
  # print(json.dumps(dataArray, indent=4))

  return dataArray