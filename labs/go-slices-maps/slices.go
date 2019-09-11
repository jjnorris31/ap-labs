package main

import "golang.org/x/tour/pic"

func Pic(dx, dy int) [][]uint8 {
	dy_slice := make([][]uint8, dy)

		for y := range dy_slice {
			dy_slice[y] = make([]uint8, dx)
			for x := range dy_slice[y] {
				v := ((x + y) / 2)
				dy_slice[y][x] = uint8(v)
			}
		}
		return dy_slice
}

func main() {
	pic.Show(Pic)
}

