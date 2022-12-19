package main

import (
	"fmt"
	"os"
	"strconv"
	"sync"
)

var glob = 0
var loops = 10000000
var mut sync.Mutex

func increment(wg *sync.WaitGroup) {
	defer wg.Done()
	for i := 0; i < loops; i++ {
		mut.Lock()
		loc := glob
		loc++
		glob = loc
		mut.Unlock()
	}
}

func main() {
	if len(os.Args[1:]) == 1 {
		loops, _ = strconv.Atoi(os.Args[1])
	}

	conc := 2
	var wg sync.WaitGroup

	wg.Add(conc)
	for i := 0; i < conc; i++ {
		go increment(&wg)
	}

	wg.Wait()

	fmt.Printf("glob = %d\n", glob)
}
