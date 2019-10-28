// Copyright © 2016 Alan A. A. Donovan & Brian W. Kernighan.
// License: https://creativecommons.org/licenses/by-nc-sa/4.0/

// See page 227.

// Netcat is a simple read/write client for TCP servers.
package main

import (
	"flag"
	"io"
	"log"
	"net"
	"os"
	"strings"
)

var (
	prefixName io.Reader
)

//!+
func main() {
	clientName := flag.String("user", "default-user", "")
	server := flag.String("server", "localhost:8000", "")
	flag.Parse()
	if ok := strings.TrimSpace(*clientName) == ""; ok {
		log.Panic("client: clientName cannot be an \" \"")
	}

	conn, err := net.Dial("tcp", *server)
	if err != nil {
		log.Fatal(err)
	}
	_, _ = io.WriteString(conn, "<meta>"+*clientName+"\n")
	done := make(chan struct{})
	go func() {
		_, _ = io.Copy(os.Stdout, conn) // NOTE: ignoring errors
		log.Println("Disconnected")
		done <- struct{}{} // signal the main goroutine
	}()
	mustCopy(conn, os.Stdin)
	_ = conn.Close()
	<-done // wait for background goroutine to finish
}

//!-

func mustCopy(dst io.Writer, src io.Reader) {
	if _, err := io.Copy(dst, src); err != nil {
		log.Fatal(err)
	}
}
