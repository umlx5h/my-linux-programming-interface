package main

import (
	"fmt"
	"time"
)

func main() {
	// fmt.Println("before sleep")
	// // 裏でnanosleep(2)を呼び出す
	time.Sleep(time.Second)
	// fmt.Println("after sleep")

	// // 裏でnanosleep(2)を呼び出す
	ticker := time.Tick(time.Millisecond * 250)

	for i := 0; i < 4; i++ {
		select {
		case <-ticker:
			fmt.Println("tick")
		}
	}
}
