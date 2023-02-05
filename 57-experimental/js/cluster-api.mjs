/**
 * socketpair(2)を親プロセスで実行した後にforkして
 * recvmsg(2)を使ってプロセス間通信していた
 *
 * socketpair(AF_UNIX, SOCK_STREAM|SOCK_CLOEXEC, 0, [21, 22])
 *   - 親プロセスでsocketpairを使ってストリーム型のUNIXドメインソケットを作成していた
 *
 */
import cluster from "node:cluster";
import http from "node:http";
import { cpus } from "node:os";
import process from "node:process";

// const numCPUs = cpus().length;
const numCPUs = 2;

if (cluster.isPrimary) {
  console.log(`Primary ${process.pid} is running`);

  // Fork workers.
  for (let i = 0; i < numCPUs; i++) {
    cluster.fork();
  }

  cluster.on("exit", (worker, code, signal) => {
    console.log(`worker ${worker.process.pid} died`);
  });
} else {
  // Workers can share any TCP connection
  // In this case it is an HTTP server
  http
    .createServer((req, res) => {
      res.writeHead(200);
      res.end("hello world\n");
    })
    .listen(8000);

  console.log(`Worker ${process.pid} started`);
}
