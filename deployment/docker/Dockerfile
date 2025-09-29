# DarkAge Game Server Dockerfile
# Multi-stage build for optimized production deployment

# Build stage
FROM ubuntu:22.04 AS builder

# Set environment variables
ENV DEBIAN_FRONTEND=noninteractive
ENV UE_VERSION=5.3
ENV PROJECT_NAME=DarkAge

# Install build dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    clang \
    cmake \
    git \
    wget \
    curl \
    unzip \
    python3 \
    python3-pip \
    mono-complete \
    && rm -rf /var/lib/apt/lists/*

# Create build user
RUN useradd -m -s /bin/bash builder
USER builder
WORKDIR /home/builder

# Copy source code
COPY --chown=builder:builder . /home/builder/DarkAge/

# Build the project (simulated - would require UE5 installation)
WORKDIR /home/builder/DarkAge
RUN echo "Building DarkAge project..." && \
    echo "In production, this would:" && \
    echo "1. Install Unreal Engine 5.3" && \
    echo "2. Run UnrealBuildTool" && \
    echo "3. Package for Linux server" && \
    echo "4. Create distribution package" && \
    mkdir -p /home/builder/DarkAge/Binaries/Linux && \
    echo "#!/bin/bash\necho 'DarkAge Server Starting...'\nsleep infinity" > /home/builder/DarkAge/Binaries/Linux/DarkAgeServer && \
    chmod +x /home/builder/DarkAge/Binaries/Linux/DarkAgeServer

# Runtime stage
FROM ubuntu:22.04 AS runtime

# Install runtime dependencies
RUN apt-get update && apt-get install -y \
    libc6 \
    libstdc++6 \
    libssl3 \
    ca-certificates \
    curl \
    && rm -rf /var/lib/apt/lists/*

# Create game user
RUN useradd -m -s /bin/bash gameserver
USER gameserver
WORKDIR /home/gameserver

# Copy built game from builder stage
COPY --from=builder --chown=gameserver:gameserver /home/builder/DarkAge/Binaries/Linux/DarkAgeServer /home/gameserver/
COPY --from=builder --chown=gameserver:gameserver /home/builder/DarkAge/Content/ /home/gameserver/Content/
COPY --from=builder --chown=gameserver:gameserver /home/builder/DarkAge/Config/ /home/gameserver/Config/

# Create necessary directories
RUN mkdir -p /home/gameserver/Saved/Logs && \
    mkdir -p /home/gameserver/Saved/Config && \
    mkdir -p /home/gameserver/Saved/SaveGames

# Expose game ports
EXPOSE 7777/udp
EXPOSE 7778/tcp
EXPOSE 8080/tcp

# Health check
HEALTHCHECK --interval=30s --timeout=10s --start-period=60s --retries=3 \
    CMD curl -f http://localhost:8080/health || exit 1

# Set entrypoint
ENTRYPOINT ["./DarkAgeServer"]
CMD ["-log", "-server", "-port=7777"]