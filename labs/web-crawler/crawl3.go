// Copyright © 2016 Alan A. A. Donovan & Brian W. Kernighan.
// License: https://creativecommons.org/licenses/by-nc-sa/4.0/

// See page 241.

// Crawl2 crawls web links starting with the command-line arguments.
//
// This version uses a buffered channel as a counting semaphore
// to limit the number of concurrent calls to links. Extract.
//
// Crawl3 adds support for depth limiting.
//

package main

import (
	"fmt"
	"flag"
	"log"
	"gopl.io/ch5/links"
)

var depth = flag.Int("depth", 3, "crawl depth")
var tokens = make(chan struct{}, 20)

func crawl(url string, currentDepth int) map[string]int {
	fmt.Printf("crawl3: depth: %d, url: %s\n", currentDepth, url)

	if currentDepth >= *depth {
		return nil
	}

	tokens <- struct{}{} // acquire a token
	list, err := links.Extract(url)
	<-tokens // release the token

	if err != nil {
		log.Print(err)
	}

	linksUrl := make(map[string]int)

	for _, link := range list {
		linksUrl[link] = currentDepth + 1
	}

	return linksUrl
}

func main() {
	// parse flag
	flag.Parse()

	worklist := make(chan map[string]int)
	var n int // number of pending sends to worklist

	// Start with the command-line arguments.
	n++
	go func() {
		linksURL := make(map[string]int)
		for _, url := range flag.Args() {
			linksURL[url] = 1
		}
		worklist <- linksURL
	}()

	// Crawl the web concurrently.
	seen := make(map[string]bool)
	for ; n > 0; n-- {
		list := <-worklist
		for key, value := range list {
			if !seen[key] {
				seen[key] = true
				n++
				go func(link string) {
					worklist <- crawl(key, value)
				}(key)
			}
		}
	}
}
