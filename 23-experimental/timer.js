// setTimeout(() => {
//   console.log("hello");

//   // おそらくepoll_wait(2)を使ってる
//   setTimeout(() => {
//     console.log("world");
//   }, 100);
// }, 1000);

const id = setInterval(() => {
  console.log("world");
}, 100);

setTimeout(() => {
  console.log("end!");
  clearInterval(id);
}, 1000);
