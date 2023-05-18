FROM gcc:latest

WORKDIR /app

COPY . /app

RUN g++ -o compare_files main.cpp

CMD ["./compare_files"]
