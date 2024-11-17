FROM --platform=${BUILDPLATFORM:-linux/amd64} python:3.9

# Avoid interactive prompts during installation
ENV DEBIAN_FRONTEND=noninteractive


# Install essential packages
RUN apt-get update && apt-get install -y \
    build-essential \
    zsh \
    cmake \
    g++ \
    gdb \
    libncurses5-dev \
    libpcap-dev \
    iproute2 \
    iputils-ping \
    net-tools \
    tcpdump \
    && rm -rf /var/lib/apt/lists/*

# Install Oh My Zsh
RUN sh -c "$(curl -fsSL https://raw.githubusercontent.com/ohmyzsh/ohmyzsh/master/tools/install.sh)" "" --unattended

# Set zsh as the default shell
SHELL ["/bin/zsh", "-c"]
# Create working directory
WORKDIR /app

# Copy your source code
COPY . .

# Build your application
RUN make

CMD ["/bin/bash"]
