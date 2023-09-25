import pika
import pymongo
import json

from config import rmqhost, rmqport, rmqvhost, rmqusername, rmqpassword, mongodburi

def consume_message(ch, method, properties, body):
    print("Received JSON data:", body)
    json_str = body.decode()
    print(json_str)

    data = json.loads(json_str)

    device, current, timestamp = data.get('device'), data.get('current_mA'), data.get('timestamp')

    if isinstance(current, (int, float)):
        print(f"Received message: Device: {device}, Current: {current}, Timestamp: {timestamp}")

        client = pymongo.MongoClient(mongodburi)
        db = client['measurement']
        collection = db['current']
        document = {'device': device, 'current': current, 'timestamp': timestamp}
        collection.insert_one(document)
    else:
        print("Invalid JSON format or missing 'current' field.")

def main():
    connection = pika.BlockingConnection(pika.ConnectionParameters(rmqhost, rmqport, rmqvhost, credentials=pika.credentials.PlainCredentials(rmqusername, rmqpassword)))
    channel = connection.channel()
    
    result = channel.queue_declare('', exclusive=True)
    queue_name = result.method.queue

    channel.queue_bind(exchange='amq.topic', queue=queue_name, routing_key='motor.*')

    channel.basic_consume(queue=queue_name, on_message_callback=consume_message, auto_ack=True)

    print('Started consuming')

    channel.start_consuming()

if __name__ == '__main__':
    main()
