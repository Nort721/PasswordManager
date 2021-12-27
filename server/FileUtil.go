package server

import (
	"bufio"
	"errors"
	"fmt"
	"log"
	"os"
	"strings"
)

/* loads the text file thats in the given path
and reads all the text in it, returns
a string slice with all the text from the file */
func ScanFile(path string) string {

	// support for unix systems
	// unix systems paste file paths with apostrophe
	if strings.Contains(path, "'") {
		path = strings.Trim(path, "'")
	}

	// open the file
	f, err := os.Open(path)

	// error handling
	if err != nil {
		log.Fatal(err)
	}

	defer f.Close()

	scanner := bufio.NewScanner(f)

	var lines []string

	for scanner.Scan() {
		lines = append(lines, scanner.Text())
	}

	// error handling
	if err := scanner.Err(); err != nil {
		log.Fatal(err)
	}

	return lines[0]
}

/* This checks if the file exists */
func Exists(path string) bool {
	// support for unix systems
	// unix systems paste file paths with apostrophe
	if strings.Contains(path, "'") {
		path = strings.Trim(path, "'")
	}

	if _, err := os.Stat(path); errors.Is(err, os.ErrNotExist) {
		return false
	}

	return true
}

func WriteFile(name string, text string) {
	f, err := os.Create(name + ".txt")

	if err != nil {
		log.Fatal(err)
	}

	defer f.Close()

	// writing it all as one line as this is how the encrypted data is saved
	_, err2 := f.WriteString(text)

	if err2 != nil {
		log.Fatal(err2)
	}
}

func DeleteFile(path string) {
	var err = os.Remove(path)

	if err != nil {
		fmt.Println(err.Error())
		return
	}

	fmt.Println("File Deleted")
}
