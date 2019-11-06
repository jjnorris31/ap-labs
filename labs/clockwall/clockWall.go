package main

import (
	"bufio"
	"fmt"
	"io"
	"log"
	"net"
	"os"
	"strings"
	"time"
)

type clock struct {
	timezone, host string
}

func (c *clock) watch(writer io.Writer, reader io.Reader) {
	s := bufio.NewScanner(reader)
	for s.Scan() {
		fmt.Fprintf(writer, "%s: %s\n", c.timezone, s.Text())
	}
	if s.Err() != nil {
		log.Printf("can't read from %s: %s", c.timezone, s.Err())
	}
}

func main() {

	if len(os.Args) < 2 {
		fmt.Fprintln(os.Stderr, "clockwall: how to run -> clockwall NewYork=localhost:8010")
		os.Exit(1)
	}

	tzs := os.Args[1:]
	clocks := make([]*clock, 0)

	for _, arg := range tzs {
	    // split the timezone and host
		str := strings.Split(arg, "=")
        timezone := str[0]
        host := str[1]


		if len(str) != 2 {
			fmt.Fprintf(os.Stderr, "clockwall: the correct format to the host is --> timezone=host%s\n", arg)
			os.Exit(1)
		}

		// append the new clock passed by the user
		clocks = append(clocks, &clock{timezone, host})
	}

	for _, clock := range clocks {
		conn, err := net.Dial("tcp", clock.host)

		if err != nil {
			log.Fatal(err)
		}

		defer conn.Close()
		go clock.watch(os.Stdout, conn)
	}

	for {
		time.Sleep(time.Minute)
	}
}