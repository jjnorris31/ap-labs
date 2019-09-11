package main

import (
	"golang.org/x/tour/wc"
	"strings"
)

func WordCount(s string) map[string]int {
	words := strings.Fields(s)
	myMap := make(map[string]int)
	
	for j := 0; j < len(words); j++ {
		_, ok := myMap[words[j]]
		if ok {
			myMap[words[j]]++
		} else {
			myMap[words[j]] = 1
		}
	}
	return myMap
}

func main() {
	wc.Test(WordCount)
}

