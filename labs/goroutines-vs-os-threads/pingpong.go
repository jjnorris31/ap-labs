package main

import (
    "os"
    "fmt"
    "os/signal"
)

var counter int64

func main() {
    ping := make(chan int)

    go func() {
        ping <- 1
        for {
            counter++
            fmt.Println(counter)
            // ping pong with one goroutine
            ping <- <- ping
        }
    }()

    go func() {
        for {
            ping <- <- ping
        }
    }()

    c := make(chan os.Signal, 1)
   	signal.Notify(c, os.Interrupt)
   	<-c

   	fmt.Println(counter, "hihi")



}