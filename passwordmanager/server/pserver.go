package main

import (
	"bufio"
	"crypto/md5"
	"crypto/sha1"
	"crypto/sha256"
	"crypto/sha512"
	"encoding/hex"
	"fmt"
	"net"
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

		//fmt.Println("Message Received: ", string(message))

		onPacketReceive(message, socket)
	}
}

func onPacketReceive(msg string, connection net.Conn) {

	var authKeySaved string = "norttest"

	// decrypt msg here

	args := strings.Split(msg, "|")

	action := args[0]
	authKey := args[1]

	if action == "vaultRequest" {

		if authKey == authKeySaved {

			// send vault to client
			fmt.Println("sending vault to client")
			connection.Write([]byte("user1:pass1|user2:pass2|user3:pass3"))

		} else {
			fmt.Println("wrong login credentials")
			connection.Write([]byte("wrong login credentials"))
		}

	}
}

/* hashing system */
// interface example --->
type Generator interface {
	Hash(text string) string
}

// this will not accept the argument if the func is not implemented
func Run_Hash(gen Generator, text string) string {
	return gen.Hash(text)
}

type HashSha1 struct{}

func (gen HashSha1) Hash(text string) string {
	hash := sha1.Sum([]byte(text))
	return hex.EncodeToString(hash[:])
}

type HashSha256 struct{}

func (gen HashSha256) Hash(text string) string {
	hasher := sha256.New()

	hasher.Write([]byte(text))

	return hex.EncodeToString(hasher.Sum(nil))
}

type HashSha384 struct{}

func (gen HashSha384) Hash(text string) string {
	hasher := sha512.New384()

	hasher.Write([]byte(text))

	return hex.EncodeToString(hasher.Sum(nil))
}

type HashSha512 struct{}

func (gen HashSha512) Hash(text string) string {
	hasher := sha512.New()

	hasher.Write([]byte(text))

	return hex.EncodeToString(hasher.Sum(nil))
}

type HashMd5 struct{}

func (gen HashMd5) Hash(text string) string {
	hash := md5.Sum([]byte(text))
	return hex.EncodeToString(hash[:])
}

// generates the sha1 hash of a given string
func GenerateSha1(text string) string {
	hash := sha1.Sum([]byte(text))
	return hex.EncodeToString(hash[:])
}

// generates the sha256 hash of a given string
func GenerateSha256(text string) string {
	hasher := sha256.New()

	hasher.Write([]byte(text))

	return hex.EncodeToString(hasher.Sum(nil))
}

// generates the sha256 hash of a given string
func GenerateSha512(text string) string {
	hasher := sha512.New()

	hasher.Write([]byte(text))

	return hex.EncodeToString(hasher.Sum(nil))
}

// generates the md5 hash of a given string
func GenerateMD5(text string) string {
	hash := md5.Sum([]byte(text))
	return hex.EncodeToString(hash[:])
}
