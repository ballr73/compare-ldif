FROM gcc:latest

WORKDIR /app

COPY main.cpp /app

RUN g++ -o cldif main.cpp

ENTRYPOINT ["./cldif"]
