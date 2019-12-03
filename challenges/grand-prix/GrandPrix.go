package main

import (
	"flag"
	"fmt"
	"github.com/faiface/pixel/text"
	"github.com/golang/freetype/truetype"
	"golang.org/x/image/font"
	"image"
	_ "image/png"
	"io/ioutil"
	"math/rand"
	"os"
	"sort"
	"time"

	"github.com/faiface/pixel"
	"github.com/faiface/pixel/pixelgl"
	"golang.org/x/image/colornames"
)

type Car struct {
	id int
	currentLap int
	speed chan int
	xPos chan int
	sprite *pixel.Sprite
	mat pixel.Matrix
	seed rand.Source
	minX int
	maxX int
	minY int
	maxY int
	crashed bool
	finished bool
	position int
	finalPosition int
	timeElapsed time.Duration
}

// basically a new main
func run() {
	// all of our code will be fired up from here

	// set the actual window
	cfg := pixelgl.WindowConfig{
		Title:  "Grand Prix",
		// creates a new rectangle
		Bounds: pixel.R(0, 0, 400, 800),
		VSync: true,
	}

	// create an actual window
	win, err := pixelgl.NewWindow(cfg)
	if err != nil {
		panic(err)
	}

	// loads a pixel font
	face, err := loadTTF("munro.ttf", 15)
	if err != nil {
		panic(err)
	}

	// loads a second pixel font
	face2, err := loadTTF("munro.ttf", 20)
	if err != nil {
		panic(err)
	}

	// race info in screen
	basicAtlas := text.NewAtlas(face, text.ASCII)
	basicTxt := text.New(pixel.V(25, 700), basicAtlas)
	basicTxt.Color = colornames.White

	// winner info in screen
	winnerAtlas := text.NewAtlas(face2, text.ASCII)
	winnerTxt := text.New(pixel.V(50, 500), winnerAtlas)

	win.SetSmooth(true)

	// loads the sprites
	pic, err := loadPicture("car.png")
	goal, goalErr := loadPicture("goal.png")
	track, trackErr := loadPicture("race.png")
	final, finalErr := loadPicture("finalpic.png")

	// checks the error
	if err != nil {
		panic(err)
	}
	if goalErr != nil {
		panic(err)
	}
	if trackErr != nil {
		panic(err)
	}
	if finalErr != nil {
		panic(err)
	}

	finalSprite := pixel.NewSprite(final, final.Bounds())

	miliSpace := 3 / totalCars
	xSpace := 400 / (totalCars + 1)
	initMiliSpace := 0
	initXSpace := xSpace

	// dummy value to init the duration property of a car
	dummyDuration, _ := time.ParseDuration("1s")

	// initialize all the cars
	for i := 0; i < totalCars; i++ {
		cars = append(cars, Car{i, 0, make(chan int), make(chan int), pixel.NewSprite(pic, pic.Bounds()), pixel.IM.Moved(pixel.V(float64(initXSpace), 0)), rand.NewSource(time.Now().UnixNano() + int64(initMiliSpace)), initXSpace - 13, initXSpace + 13, - 22, 22, false, false, 0, 0, dummyDuration},)
		tempTotalSpeed = append(tempTotalSpeed, 0)
		tempXPos = append(tempXPos, 0)
		initMiliSpace += miliSpace
		initXSpace += xSpace
	}

	win.Clear(colornames.White)

	for index := range cars {
		cars[index].getSpeed(cars[index].seed)
		cars[index].getXPos(cars[index].seed)
	}

	start := time.Now()
	// main loop to keep the window up and running
	// until a user clicks the close button
	for !win.Closed() {

		// if 3 cars finished already do
		if finalTop == 3 {
			// clear all the previous elements of the window
			win.Clear(colornames.Black)
			winnerTxt.Clear()
			basicTxt.Clear()

			// do some format to the text
			finalSprite.Draw(win, pixel.IM.Moved(win.Bounds().Center()))
			winnerTxt.LineHeight = winnerAtlas.LineHeight() * 1.5
			winnerTxt.Color = colornames.Black

			for i := 0; i < finalTop; i++ {
				_, _ = fmt.Fprintf(winnerTxt, "Top: %d | ID Car: %d | Total time: %.4v\n", top[i].finalPosition, top[i].id + 1, top[i].timeElapsed)
			}
			winnerTxt.Draw(win, pixel.IM)
		} else {

			// clear the window everytime
			positionArray = positionArray[:0]
			basicTxt.Clear()

			for i := 0; i < totalCars; i++ {
				tempCurrentSpeed :=  <- cars[i].speed
				if !cars[i].crashed {
					tempXPos[i] += <- cars[i].xPos
				} else {
					tempXPos[i] = 0
					tempCurrentSpeed = -1
					go disableAndReplace(i)
				}
				tempTotalSpeed[i] += tempCurrentSpeed

				// update the info of the race in the screen
				if !cars[i].finished {
					_, _ = fmt.Fprintf(basicTxt, "Car: %d | %d km/h | current Lap: %d | pos: %d\n", cars[i].id + 1, tempCurrentSpeed * 10 ,cars[i].currentLap, cars[i].position)
				} else {
					_, _ = fmt.Fprintf(basicTxt, "Car: %d | Top pos: %d | Total time: %.4v\n", cars[i].id + 1, cars[i].finalPosition, cars[i].timeElapsed)
				}
			}

			// position array
			for i := 0; i < totalCars; i++ {
				positionArray = append(positionArray, tempTotalSpeed[i])
			}
			sort.Ints(positionArray)

			tempIndex := totalCars
			for i := 0; i < totalCars; i++ {
				for j := 0; j < totalCars; j++ {
					if positionArray[i] == tempTotalSpeed[j] {
						cars[j].position = tempIndex - i + finalTop
					}
				}
			}

			// win.Bounds returns a rectangle, the bounds of the window getting the center
			trackSprite := pixel.NewSprite(track, track.Bounds())
			trackSprite.Draw(win, pixel.IM.Moved(win.Bounds().Center()))
			goalSprite := pixel.NewSprite(goal, goal.Bounds())
			goalSprite.Draw(win, pixel.IM.Moved(pixel.V(200, 808)))

			tempInitPosition := xSpace
			for i := 0; i < totalCars; i++ {
				tempXPosition := tempInitPosition + tempXPos[i]
				if tempXPosition < 0 {
					tempXPosition = 20
				} else if tempXPosition > 400 {
					tempXPosition = 380
				}

				tempVector := pixel.V(float64(tempXPosition), float64(tempTotalSpeed[i]))
				cars[i].mat = pixel.IM.Moved(tempVector)

				cars[i].minX = tempInitPosition + tempXPos[i] - 13
				cars[i].maxX = tempInitPosition + tempXPos[i] + 13
				cars[i].minY = tempTotalSpeed[i] - 22
				cars[i].maxY = tempTotalSpeed[i] + 22
				tempInitPosition += xSpace

				// check if a car crashed with other
				cars[i].checkCrash(i)
				// redraw all cars
				cars[i].sprite.Draw(win, cars[i].mat)

				if tempTotalSpeed[i] > 800 {
					tempTotalSpeed[i] = 0
					cars[i].passLap(i)

					if cars[i].currentLap == laps && !cars[i].finished{
						cars[i].finished = true
						cars[i].timeElapsed = time.Since(start)
						finalTop++
						cars[i].finalPosition = finalTop
						top = append(top, cars[i])
						close(cars[i].xPos)
						close(cars[i].speed)
					}
				}
			}
		}

		// redraws the window every time (but doesn't erase it's content).

		if finalTop < 4 {
			basicTxt.Draw(win, pixel.IM)
		}
		// update the frame
		win.Update()
	}

}

var totalCars int
var cars []Car
var tempTotalSpeed []int
var tempXPos []int
var laps int
var positionArray []int
var finalTop int
var top []Car

func main() {
	tempTotal := flag.Int("number", 10, "numbers of cars")
	tempLaps := flag.Int("laps", 1, "numbers of laps")
	flag.Parse()
	totalCars = *tempTotal
	laps = *tempLaps
	finalTop = 0
	// puts PixelGL in control og the main function
	pixelgl.Run(run)
}

/**
 / loads an image to convert to an sprite
 */
func loadPicture(path string) (pixel.Picture, error) {
	// opens a file
	file, err := os.Open(path)

	if err != nil {
		return nil, err
	}

	defer file.Close()

	img, _, err := image.Decode(file)
	if err != nil {
		return nil, err
	}
	return pixel.PictureDataFromImage(img), nil
}

// gorutina que obtiene una velocidad random en el eje Y
// en el eje Y
func getRndSpeed(rnd chan int, source rand.Source) {
	r := rand.New(source)
	a := 0
	b := 7
	for  {
		time.Sleep(50 * time.Millisecond)
		select {
		case <- rnd:
			return
		default:
			rnd <- a + r.Intn(b - a + 1)
		}
	}
}

// gorutina que obtiene una posición random en el eje X
func getRndXPos(rnd chan int, source rand.Source) {
	a := -3
	b := 3
	r := rand.New(source)
	for  {
		time.Sleep(50 * time.Millisecond)
		select {
		case <- rnd:
			fmt.Println("finished goroutine")
			return
		default:
			rnd <- a + r.Intn(b - a + 1)
		}
	}
}

func (car Car) getSpeed(timeInit rand.Source) {
	go getRndSpeed(car.speed, timeInit)
}

func (car Car) getXPos(timeInit rand.Source) {
	go getRndXPos(car.xPos, timeInit)
}

func (car Car) passLap(i int) {
	cars[i].currentLap++
}

func (car Car) checkCrash(i int) {
	for j := 0; j < totalCars; j++ {
		if i != j {
			if car.minX < cars[j].maxX &&
				car.maxX > cars[j].minX &&
				car.minY < cars[j].maxY &&
				car.maxY > cars[j].minY {
				cars[i].crashed = true
			}
		}
	}
}

func disableAndReplace(i int) {
	time.Sleep(2000 * time.Millisecond)
	cars[i].crashed = false
}

func loadTTF(path string, size float64) (font.Face, error) {
	file, err := os.Open(path)
	if err != nil {
		return nil, err
	}
	defer file.Close()

	bytes, err := ioutil.ReadAll(file)
	if err != nil {
		return nil, err
	}

	font2, err := truetype.Parse(bytes)
	if err != nil {
		return nil, err
	}

	return truetype.NewFace(font2, &truetype.Options{
		Size:              size,
		GlyphCacheEntries: 1,
	}), nil
}



