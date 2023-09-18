import pika
import pymongo
import json
import re  # Import the regular expression module

# Import the credential variables from the file
from config import rmqhost, rmqport, rmqvhost, rmqusername, rmqpassword, mongodburi

def remove_control_characters(s):
    # Remove control characters from a string using regex
    control_char_regex = re.compile(r'[\x00-\x1F\x7F-\x9F]+')
    return control_char_regex.sub('', s)

def main():
    try:
        connection = pika.BlockingConnection(pika.ConnectionParameters(rmqhost, rmqport, rmqvhost, credentials=pika.credentials.PlainCredentials(rmqusername, rmqpassword)))
        channel = connection.channel()

        # Declare the exchange as 'amqp.topic'
        # channel.exchange_declare(exchange='amqp.topic', exchange_type='topic')

        # Create a unique queue for this consumer
        result = channel.queue_declare('', exclusive=True)
        queue_name = result.method.queue

        # Bind the queue to the "motor" topic
        channel.queue_bind(exchange='amq.topic', queue=queue_name, routing_key='motor')

        def callback(ch, method, properties, body):
            try:
                print("Received JSON data:", body)  # Print the raw JSON data
                json_str = body.decode()

                # Remove control characters from the "device" field
                json_str = remove_control_characters(json_str)

                data = json.loads(json_str)  # Parse the JSON data

                # Extract attributes from JSON data
                device = data.get('device')
                current = data.get('current')
                timestamp = data.get('timestamp')

                if current is not None and isinstance(current, (int, float)):
                    print(f"Received message: Device: {device}, Current: {current}, Timestamp: {timestamp}")

                    client = pymongo.MongoClient(mongodburi)
                    db = client['measurement']
                    collection = db['current']
                    document = {'device': device, 'current': current, 'timestamp': timestamp}
                    collection.insert_one(document)
                else:
                    print("Invalid JSON format or missing 'current' field.")

            except json.JSONDecodeError as e:
                print(f'Error parsing JSON: {e}')
            except Exception as e:
                print(f'Error: {e}')

        channel.basic_consume(queue=queue_name, on_message_callback=callback, auto_ack=True)

        print('Started consuming')

        try:
            channel.start_consuming()
        except KeyboardInterrupt:
            print('Interrupted')

    except pika.exceptions.AMQPError as e:
        print(f'AMQP Error: {e}')
    except pymongo.errors.PyMongoError as e:
        print(f'PyMongo Error: {e}')
    except Exception as e:
        print(f'Error: {e}')
    finally:
        if connection.is_open:
            connection.close()

if __name__ == '__main__':
    main()
