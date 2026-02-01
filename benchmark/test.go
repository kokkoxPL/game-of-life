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
	fmt.Printf("Testowanie %s:\n", name)
	runCommand(cmd, args...)
	
	var total time.Duration
	for range times {
		elapsed, err := runCommand(cmd, args...)
		if err != nil {
			fmt.Printf("%s błąd: %v\n", name, err)
			break
		}
		total += elapsed
	}

	fmt.Printf("Średni czas: %s\n\n", total/time.Duration(times))
}


func main() {
	runs := 10

	c := "./c/build/GameOfLife-cli.exe"
	cArgs := []string{
		"-s", "32", "32",
		"-g", "50",
		// "-f", "./benchmark/test_val.txt",
	} 
	benchmark("C 32x32", c, cArgs, runs)

	
	cArgsSimd := []string{
		"-s", "32", "32",
		"-g", "1",
		"--simd",
		// "-f", "./benchmark/test_val.txt",
	} 
	benchmark("C 32x32 (simd)", c, cArgsSimd, runs)


	cArgs2 := []string{
		"-s", "1024", "1024",
		"-g", "50",
		// "-f", "./benchmark/test_val.txt",
	} 
	benchmark("C 1024x1024", c, cArgs2, runs)

	
	cArgsSimd2 := []string{
		"-s", "1024", "1024",
		"-g", "50",
		"--simd",
		// "-f", "./benchmark/test_val.txt",
	} 
	benchmark("C 1024x1024 (simd)", c, cArgsSimd2, runs)

	python := []string{
		"./python/cli.py",
		"-s", "32", "32",
		"-g", "50",
		"-r", "0.4",
		"-b", "numpy",
		"-i",
	}

	benchmark("Python", "python", python, runs)

	pythonUv := []string{
    	"run",
		"./python/cli.py",
		"-s", "32", "32",
		"-g", "50",
		"-r", "0.4",
		"-b", "numpy",
		"-i",
	}

	benchmark("Python (uv)", "uv", pythonUv, runs)
}
