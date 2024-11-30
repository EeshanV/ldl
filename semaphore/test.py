import threading
import os
import time

DEVICE_FILE = '/dev/semaphore'
NUM_THREADS = 5
BUFFER_SIZE = 64

def write_to_device(thread_id):
    try:
        with open(DEVICE_FILE, 'w') as device:
            for i in range(5):
                data = f'Thread {thread_id} writes this\n'
                device.write(data)
                print(f'Thread {thread_id}: wrote to device')
                time.sleep(0.1)
    except Exception as e:
        print(f'Thread {thread_id}: Error writing to device - {e}')

def read_from_device(thread_id):
    try:
        with open(DEVICE_FILE, 'r') as device:
            for i in range(5):
                data = device.read(BUFFER_SIZE)
                print(f'Thread {thread_id}: read from device - {data.strip()}')
                time.sleep(0.1)
    except Exception as e:
        print(f'Thread {thread_id}: Error reading from device - {e}')

def main():
    threads = []

    for i in range(NUM_THREADS):
        if i % 2 == 0:
            thread = threading.Thread(target=write_to_device, args=(i,))
        else:
            thread = threading.Thread(target=read_from_device, args=(i,))
        threads.append(thread)

    for thread in threads:
        thread.start()

    for thread in threads:
        thread.join()
main()