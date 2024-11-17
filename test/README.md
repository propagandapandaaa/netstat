**TESTING**
The testing environment is created using docker.

# Start the containers
docker compose up -d

# Access the main container
docker compose exec app bash

# Access the traffic generator container
docker compose exec traffic-gen bash
