import socket
import time
import sys

def send_test_packets():
    udp_sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    
    LOOPBACK_IP = '127.0.0.1'
    UDP_PORT = 12345

    TOTAL_BYTES = 10 * 1024 * 1024  # 10 MB 
    DURATION = 10
    PACKETS_PER_SECOND = 1000
    BYTES_PER_SECOND = TOTAL_BYTES / DURATION
    PACKET_SIZE = int(BYTES_PER_SECOND / PACKETS_PER_SECOND)

    try:
        print(f"\nSending {TOTAL_BYTES/1024/1024:.1f} MB over {DURATION} seconds...")
        print(f"Packet size: {PACKET_SIZE} bytes")
        print(f"Rate: {BYTES_PER_SECOND/1024/1024:.1f} MB/s")

        start_time = time.time()
        packets_sent = 0
        bytes_sent = 0

        while time.time() - start_time < DURATION:
            current_second = int(time.time() - start_time)
            target_packets = (current_second + 1) * PACKETS_PER_SECOND
            
            while packets_sent < target_packets and time.time() - start_time < DURATION:
                packet_data = b'U' * PACKET_SIZE
                udp_sock.sendto(packet_data, (LOOPBACK_IP, UDP_PORT))
                packets_sent += 1
                bytes_sent += PACKET_SIZE

                time.sleep(1/PACKETS_PER_SECOND)

        elapsed_time = time.time() - start_time
        print(f"\nFinished sending packets:")
        print(f"Total bytes sent: {bytes_sent/1024/1024:.1f} MB")
        print(f"Actual duration: {elapsed_time:.1f} seconds")
        print(f"Actual rate: {bytes_sent/elapsed_time/1024/1024:.1f} MB/s")
            
    except socket.error as e:
        print(f"Socket error: {e}")
    except KeyboardInterrupt:
        print("\nStopping packet generation...")
    finally:
        udp_sock.close()
        print("Socket closed")

if __name__ == "__main__":
    print("Starting packet generation on loopback interface (lo0)")
    print("Interface details:")
    print("- IP: 127.0.0.1")
    print("- UDP Port: 12345")
    
    send_test_packets()
