FROM silkeh/clang:11-bullseye

#set timezone to europe/madrid
RUN ln -fs /usr/share/zoneinfo/Europe/Madrid /etc/localtime

RUN apt update && apt install -y \
    git \
    cmake \
    clang-format \
    clang-tools \
    python3 \
    python3-pip \
    valgrind \
    && apt-get clean && rm -rf /var/lib/apt/lists/*

RUN pip3 install cpplint

# install selenium and firefox driver
# Download and install geckodriver
ENV GECKODRIVER_VERSION 0.30.0
RUN wget -O /tmp/geckodriver.tar.gz https://github.com/mozilla/geckodriver/releases/download/v${GECKODRIVER_VERSION}/geckodriver-v${GECKODRIVER_VERSION}-linux64.tar.gz && \
    tar -C /opt -xzf /tmp/geckodriver.tar.gz && \
    chmod +x /opt/geckodriver && \
    ln -s /opt/geckodriver /usr/local/bin/geckodriver && \
    rm /tmp/geckodriver.tar.gz
RUN apt update && apt-get install firefox-esr -y
RUN pip3 install selenium


