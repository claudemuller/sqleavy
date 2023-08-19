package tests

import (
	"bytes"
	"fmt"
	"os/exec"
	"strings"
)

func runTest(cmds, expected []string) error {
	var stdout, stderr bytes.Buffer

	cmd := exec.Command("../bin/sqleavy")
	cmdsStr := strings.Join(cmds, "")
	cmd.Stdin = bytes.NewBuffer([]byte(cmdsStr))
	cmd.Stdout = &stdout
	cmd.Stderr = &stderr

	if err := cmd.Run(); err != nil {
		return fmt.Errorf("cmd.Run error: \"%s\"\n", string(stderr.Bytes()))
	}

	out := strings.SplitAfter(string(stdout.Bytes()), "\n")
	out = out[4:]

	if len(expected) != len(out) {
		fmt.Println(out)
		return fmt.Errorf("incorrect number of outputs: expected %d, got %d", len(expected), len(out))
	}

	for i := 0; i < len(out); i++ {
		o := strings.TrimSpace(out[i])
		e := strings.TrimSpace(expected[i])
		if strings.Compare(o, e) != 0 {
			return fmt.Errorf("at: \"%s\" not equal to \"%s\"\n", o, e)
		}
		if stderr.Len() > 0 {
			return fmt.Errorf("error: %v", string(stderr.Bytes()))
		}
	}

	return nil
}

func ReportFailure(name string, err error) {
	fmt.Printf("❌ Test \"%s\" failed: %v\n", name, err)
}

func ReportSuccess(name string) {
	fmt.Printf("✅ %s tests passed.\n", name)
}
