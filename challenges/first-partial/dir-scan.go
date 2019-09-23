package main

import (
	"fmt"
	"os"
	"path/filepath"
	"text/tabwriter"
)

var directories = 0
var symbolic = 0
var sockets = 0
var others = 0
var devices = 0

func scanDir(dir string) error {
	return filepath.Walk(dir, func(somePath string, info os.FileInfo, e error) error {
		if e != nil {
			return e
		}

		// abbreviation for Mode().IsDir()
		if info.IsDir() {
			// d: is a directory
			directories++
		} else if info.Mode() & os.ModeSymlink != 0 {
				// L: symbolic link
				symbolic++
		} else if info.Mode() & os.ModeSocket != 0 {
			// S: unix domain socket
			sockets++
		} else if info.Mode() & os.ModeDevice != 0 {
			// D: device file
			devices++
		} else if info.Mode().IsRegular() {
				// reports whether m describes a regular file
				others++
		}
		return nil
	})
}

func main() {

	if len(os.Args) < 2 {
		fmt.Println("U need to send the correct number of arguments...")
		fmt.Println("Usage: ./dir-scan <directory>")
		os.Exit(1)
	} else {

		err := scanDir(os.Args[1])

		if err != nil {
			fmt.Printf("something went wrong: %v", err)
			panic(err)
		}

		w := new(tabwriter.Writer)
		fmt.Println("Directory Scanner Tool")
		fmt.Println("")
		w.Init(os.Stdout, 0, 0, 20, ' ', tabwriter.Debug|tabwriter.AlignRight)
		fmt.Fprintln(w, "Path\t", os.Args[1],"\t")
		fmt.Fprintln(w, "Directories\t",directories,"\t")
		fmt.Fprintln(w, "Symlinks\t", symbolic,"\t")
		fmt.Fprintln(w, "Devices\t", devices,"\t")
		fmt.Fprintln(w, "Sockets\t", sockets,"\t")
		fmt.Fprintln(w, "Others\t", others,"\t")
		w.Flush()
		}

}