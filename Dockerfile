FROM debian/eol:etch-slim@sha256:414f34489996a6455a4b799ddfab5867165f6d03041f746c39facaf7838c33a6

RUN apt-get update && apt-get install -y g++-2.95 make \
    && apt-get clean && rm -r /var/lib/apt/lists/*

WORKDIR /mnt/build
