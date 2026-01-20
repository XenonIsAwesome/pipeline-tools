import time
from argparse import ArgumentParser

import pika

if __name__ == "__main__":
    argparser = ArgumentParser("send_mq")
    argparser.add_argument("-q", "--queue", type=str, dest="queue", required=True,
                           help="The RabbitMQ queue to publish to")
    argparser.add_argument("-e", "--exchange", type=str, dest="exchange", required=True,
                           help="The RabbitMQ exchange to publish to")
    argparser.add_argument("--delay", type=float, dest="delay", required=False, default=1,
                           help="The time between publications of the content to the RabbitMQ channel")
    argparser.add_argument("message", type=str,
                           help="The content to publish to the RabbitMQ channel.")

    args = argparser.parse_args()

    connection = pika.BlockingConnection(
        pika.ConnectionParameters(host='127.0.0.1', credentials=pika.PlainCredentials("guest", "guest"))
    )
    channel = connection.channel()

    channel.exchange_declare(exchange=args.exchange, exchange_type='fanout')
    channel.queue_declare(queue=args.queue)
    channel.queue_bind(exchange=args.exchange, queue=args.queue, routing_key=args.queue)

    while True:
        try:
            channel.basic_publish(exchange=args.exchange, routing_key=args.queue, body=args.message)
            print(f" [x] Sent {args.message}")
        except Exception as e:
            print(f"  [err] {e}")
        except KeyboardInterrupt:
            break
        time.sleep(args.delay)

    connection.close()
