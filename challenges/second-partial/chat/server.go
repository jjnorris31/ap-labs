// Copyright © 2016 Alan A. A. Donovan & Brian W. Kernighan.
// License: https://creativecommons.org/licenses/by-nc-sa/4.0/

// See page 254.
//!+

// Chat is a server that lets clients chat with each other.
package main

import (
	"bufio"
	"flag"
	"fmt"
	"log"
	"net"
	"regexp"
	"strings"
	"time"
)

//!+broadcaster
type client chan<-string // an outgoing message channel

//!+user
type user struct {
	connection net.Conn
	admin bool
	ch client
}

var (
	entering = make(chan client)
	leaving  = make(chan client)
	messages = make(chan string) // all incoming client messages
	users = make(map[string]user)
)

func broadcaster() {
	clients := make(map[client]bool) // all connected clients
	for {
		select {
		case msg := <-messages:
			// Broadcast incoming message to all
			// clients' outgoing message channels.
			for cli := range clients {
				cli <- msg
			}

		case cli := <-entering:
			clients[cli] = true

		case cli := <-leaving:
			delete(clients, cli)
			close(cli)
		}
	}
}

//!-broadcaster

//!+handleConn
func handleConn(conn net.Conn) {
	ch := make(chan string) // outgoing client messages
	go clientWriter(conn, ch)
	who := ""

	input := bufio.NewScanner(conn)
	for input.Scan() {
		msg := input.Text()

		if strings.Compare(msg, "/users") == 0 {
			for key, _ := range users {
				_, _ = fmt.Fprintf(conn, "| %s | \n", key)
			}
		} else if strings.Compare(msg, "/time") == 0 {
			_, _ = fmt.Fprintf(conn, "irc-server > %s\n", time.Now().Format("Mon Jan _2 15:04:05 2006"))
		} else if matches, _ := regexp.MatchString("^/msg .+ .+", msg); matches {
			stringSlice := strings.Split(msg, " ")
			lenSlice := len(stringSlice)
			if user, ok := users[stringSlice[1]]; ok {
				_, _ = fmt.Fprintf(user.connection, "%s [privatemsg] > ", who)
				for i := 2; i < lenSlice; i++ {
					_, _ = fmt.Fprintf(user.connection, "%s ", stringSlice[i])
				}
				_, _ = fmt.Fprintln(user.connection, "")
			} else {
				_, _ = fmt.Fprintf(conn, "irc-server > No such user; use /users to see actual users\n")
			}
		} else if matches, _ := regexp.MatchString("^/user .+$", msg); matches {
			stringSlice := strings.Split(msg, " ")
			if _, ok := users[stringSlice[1]]; ok {
				_, _ = fmt.Fprintf(conn, "irc-server > name: %s, ip: %s\n", stringSlice[1], users[stringSlice[1]].connection.RemoteAddr().String())
			} else {
				_, _ = fmt.Fprintf(conn, "irc-server > No such user; use /users to see actual users\n")
			}
		} else if matches, _ := regexp.MatchString("^<meta>.+", msg); matches {
			stringSlice := strings.Split(msg, ">")
			who = stringSlice[1]
			messages <- "irc-server > [" + who + "] has arrived"
			users[who] = user{conn, false, ch}
			ch <- "irc-server > Your user [" + who + "] is successfully logged"
			entering <- ch
			fmt.Printf("irc-server > New connected user [%s]\n", who)
			// set the first user
			if len(users) == 1 {
				users[who] = user{conn, true, ch}
				ch <- "Congrats, you were the first user."
				ch <- "You're the new IRC Server ADMIN"
				fmt.Printf("[%s] was promoted as the channel ADMIN\n", who)
			}
		} else if matches, _ := regexp.MatchString("^/kick .+$", msg); matches{
			stringSlice := strings.Split(msg, " ")
			if tempUser, ok := users[stringSlice[1]]; ok {
				if !users[who].admin {
					ch <- "irc-server > You do not have sufficient permissions to kick an user"
				} else {
					messages <- "irc-server > [" + stringSlice[1] + "] was kicked from channel for bad language policy violation"
					_, _ = fmt.Fprintf(tempUser.connection, "You're kicked from this channel\n")
					_, _ = fmt.Fprintf(tempUser.connection, "Bad language is not allowed on this channel\n")
					_ = tempUser.connection.Close()
					delete(users, stringSlice[1])
				}
			} else {
				_, _ = fmt.Fprintf(conn, "irc-server > No such user; use /users to see actual users\n")
			}
		} else {
			messages <- who + "> " + msg
		}
	}
	// NOTE: ignoring potential errors from input.Err()

	ch <- "irc-server > [" + who + "] left channel"
	for key, _ := range users {
		fmt.Println(users[key].admin, len(users))
	}
	// change the admin
	if users[who].admin && len(users) != 0 {
		delete(users, who)
		for key, _ := range users {
			users[key] = user{users[key].connection, true, users[key].ch}
			users[key].ch <- "You're the new IRC Server ADMIN"
			fmt.Printf("[%s] was promoted as the channel ADMIN\n", key)
			break
		}
	}
	leaving <- ch
	messages <- "irc-server > [" + who + "] left"
	fmt.Printf("[%s] left\n", who)
	_ = conn.Close()
}

func clientWriter(conn net.Conn, ch <-chan string) {
	for msg := range ch {
		_, _ = fmt.Fprintln(conn, msg) // NOTE: ignoring network errors
	}
}

//!-handleConn

//!+main
func main() {
	host := flag.String("host", "localhost", "")
	port := flag.String("port", "8000", "")
	flag.Parse()
	listener, err := net.Listen("tcp", *host+":"+*port)
	fmt.Printf("irc-server > Simple IRC Server started at %s:%s\n", *host, *port)
	if err != nil {
		log.Fatal(err)
	}

	go broadcaster()
	for {
		conn, err := listener.Accept()
		if err != nil {
			log.Print(err)
			continue
		}
		go handleConn(conn)
	}
}

//!-main
