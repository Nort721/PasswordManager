package main

import (
	"bufio"
	"crypto/aes"
	"crypto/cipher"
	"crypto/md5"
	"crypto/rand"
	"crypto/sha256"
	"crypto/sha512"
	"encoding/hex"
	"errors"
	"fmt"
	"io"
	"log"
	"net"
	"os"
	"strconv"
	"strings"
)

func main() {
	Init()
}

func Init() {

	const version string = "v0.0.1"
	const connectionPort int = 8000

	fmt.Println("=+=------------------=+=")
	fmt.Println("PasswordManagment-Server-" + version)
	fmt.Println("=+=------------------=+=")
	fmt.Println("")
	fmt.Println("")

	server, _ := net.Listen("tcp", ":"+strconv.Itoa(connectionPort))

	fmt.Println("listening to sockets . . .")
	for {
		socket, _ := server.Accept()

		message, _ := bufio.NewReader(socket).ReadString('\n')

		fmt.Println("Message Received: ", string(message))

		onPacketReceive(message, socket)
	}
}

// hashing and salting to safely store as the vault auth key
func hashAuthKey(plainKey string) string {
	const static_salt_back = "fdjgiernorwssdklsvmlmawknqhfudsuf"
	const static_salt_front = "bjoikpdfosivhuywejraerfdshytikuyhfdgf"
	hashedKey := plainKey
	for i := 0; i < 300; i++ {
		hashedKey = GenerateSha512(static_salt_back + hashedKey + static_salt_front)
	}
	return hashedKey
}

func onPacketReceive(msg string, connection net.Conn) {

	// obviusly use SSL in the future, too lazy to set it up rn
	// we are using the md5 hash of our keypass since it always has to
	// be 32 bits
	communication_encryption_key := GenerateMD5("mysupersecretkeypass")

	// decrypt msg here
	msg = Decrypt(msg, communication_encryption_key)

	args := strings.Split(msg, "|")

	if len(args) < 2 {
		connection.Write([]byte("incorrect args"))
		return
	}

	action := args[0]
	authKey := args[1]

	authKeyHash := hashAuthKey(authKey)

	fmt.Println("processing message . . .")

	if len(args) == 2 {

		if action == "vaultRequest" {

			// can't return a vault that doesn't exist
			if !exists(authKeyHash) {
				fmt.Println("vault " + authKeyHash + " not found")
				connection.Write([]byte("vault not found"))
				return
			}

			// read vauilt from database
			vault := ScanFile(authKeyHash)

			// send vault to client
			fmt.Println("sending vault to " + authKeyHash)
			connection.Write([]byte(vault))

		}

	} else if len(args) == 3 {

		data := args[2]

		if action == "createVault" {

			/* If there is already a vault using these credentials */
			if exists(authKey) {
				connection.Write([]byte("invalid auth key, please type different credentials"))
				return
			}

			// writing new data to database
			WriteFile(authKeyHash, data)

			fmt.Println("vault created for user " + authKeyHash)
			connection.Write([]byte("new vault created!"))
		} else if action == "updateVault" {

			// can't update a vault that doesn't exist
			if !exists(authKeyHash) {
				connection.Write([]byte("vault not found"))
				return
			}

			// delete old vault
			deleteFile(authKeyHash)

			// create new vault
			WriteFile(authKeyHash, data)

			fmt.Println("vault updated for user " + authKeyHash)
			connection.Write([]byte("vault updated!"))
		}

	}
}

/* encryption code */
func Encrypt(str string, passphrase string) []byte {
	var data []byte = []byte(str)
	block, _ := aes.NewCipher([]byte(GenerateMD5(passphrase)))
	gcm, err := cipher.NewGCM(block)
	if err != nil {
		panic(err.Error())
	}
	nonce := make([]byte, gcm.NonceSize())
	if _, err = io.ReadFull(rand.Reader, nonce); err != nil {
		panic(err.Error())
	}
	ciphertext := gcm.Seal(nonce, nonce, data, nil)
	return ciphertext
}

func Decrypt(str string, passphrase string) string {
	var data []byte = []byte(str)
	key := []byte(GenerateMD5(passphrase))
	block, err := aes.NewCipher(key)
	if err != nil {
		panic(err.Error())
	}
	gcm, err := cipher.NewGCM(block)
	if err != nil {
		panic(err.Error())
	}
	nonceSize := gcm.NonceSize()
	nonce, ciphertext := data[:nonceSize], data[nonceSize:]
	plaintext, err := gcm.Open(nil, nonce, ciphertext, nil)
	if err != nil {
		panic(err.Error())
	}
	return string(plaintext)
}

/* hashing code */
func GenerateSha256(text string) string {
	hasher := sha256.New()

	hasher.Write([]byte(text))

	return hex.EncodeToString(hasher.Sum(nil))
}

func GenerateSha512(text string) string {
	hasher := sha512.New()

	hasher.Write([]byte(text))

	return hex.EncodeToString(hasher.Sum(nil))
}

func GenerateMD5(text string) string {
	hash := md5.Sum([]byte(text))
	return hex.EncodeToString(hash[:])
}

/* Flat file database */
// loads the text file thats in the given path
// and reads all the text in it, returns
// a string slice with all the text from the file
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
func exists(path string) bool {
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

func deleteFile(path string) {
	var err = os.Remove(path)

	if err != nil {
		fmt.Println(err.Error())
		return
	}

	fmt.Println("File Deleted")
}
