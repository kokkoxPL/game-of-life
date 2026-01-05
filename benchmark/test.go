package main

import (
	"fmt"
	"os/exec"
	"time"
)

func runCommand(name string, args ...string) (time.Duration, error) {
	start := time.Now()

	cmd := exec.Command(name, args...)
	cmd.Stdout = nil
	cmd.Stderr = nil

	err := cmd.Run()
	elapsed := time.Since(start)

	return elapsed, err
}

func benchmark(name string, cmd string, args []string, times int) {
	fmt.Printf("Benchmarking %s:\n", name)
	runCommand(cmd, args...)

	var total time.Duration
	for range times {
		elapsed, err := runCommand(cmd, args...)
		if err != nil {
			fmt.Printf("%s failed: %v\n", name, err)
			break
		}
		total += elapsed
	}

	fmt.Printf("Average time: %s\n\n", total/time.Duration(times))
}


func main() {
	runs := 10

	// c := "./c/main.out"
	// cArgs := []string{} 
	// benchmark("C 1", c, cArgs, runs)
	//
	// c2 := "./c/main2.out"
	// cArgs2 := []string{} 
	// benchmark("C 2", c2, cArgs2, runs)

	python := []string{
		"./python/cli.py",
		"-s", "10", "10",
		"-g", "10",
		"-f", "./python/val.txt",
		"-b",
		"-n",
	}

	benchmark("Python: ", "python", python, runs)
}
