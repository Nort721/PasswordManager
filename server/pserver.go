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
	"fmt"
	"io"
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

// the hashing that server applies to safely store the key in its database
func hashAuthKey(plainKey string) string {
	return plainKey
}

func onPacketReceive(msg string, connection net.Conn) {

	// will be saved in file in the future (maybe database at some point if I feel like it)
	var authKeySaved string = "norttest"

	// decrypt msg here

	args := strings.Split(msg, "|")

	action := args[0]
	authKey := args[1]

	if action == "vaultRequest" {

		if hashAuthKey(authKey) == authKeySaved {

			// send vault to client
			fmt.Println("sending vault to client")
			connection.Write([]byte("user1:pass1|user2:pass2|user3:pass3"))

		} else {
			fmt.Println("wrong login credentials")
			connection.Write([]byte("wrong login credentials"))
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
