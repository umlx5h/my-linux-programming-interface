package main

import (
	"fmt"
	"sync"
	"time"
)

func main() {
	var mutex sync.Mutex
	cond := sync.NewCond(&mutex)

	for _, name := range []string{"A", "B", "C"} {
		go func(name string) {
			// ロックしてからWaitメソッドを呼ぶ
			mutex.Lock()
			defer mutex.Unlock()
			// Broadcast()が呼ばれるまで待つ
			cond.Wait()
			// 呼ばれた！
			fmt.Print(name)
			time.Sleep(time.Millisecond * 100)
			fmt.Printf("!!!\n")
		}(name)
	}
	fmt.Println("よーい")
	time.Sleep(time.Second)
	fmt.Println("どん！ ")
	// 待っているgoroutineを一斉に起こす
	cond.Broadcast()
	// cond.Signal()
	time.Sleep(time.Second)

}
