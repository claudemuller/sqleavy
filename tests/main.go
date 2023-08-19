package main

import tests "github.com/claudemuller/sqleavy/test-cases"

func main() {
	if err := tests.TestHelp(); err != nil {
		tests.ReportFailure("Help", err)
		return
	}
	tests.ReportSuccess("Help")

	if err := tests.TestInsert(); err != nil {
		tests.ReportFailure("Insert", err)
		return
	}
	tests.ReportSuccess("Insert")

	if err := tests.TestInsert1400(); err != nil {
		tests.ReportFailure("Insert 1400 rows", err)
		return
	}
	tests.ReportSuccess("Insert 1400 rows")

	if err := tests.TestInsert1401(); err != nil {
		tests.ReportFailure("Insert 1401 rows", err)
		return
	}
	tests.ReportSuccess("Insert 1401 rows")

	if err := tests.TestSelect(); err != nil {
		tests.ReportFailure("Select", err)
		return
	}
	tests.ReportSuccess("Select")
}
