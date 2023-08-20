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

	if err := tests.TestInsert1300(); err != nil {
		tests.ReportFailure("Insert 1300 rows", err)
		return
	}
	tests.ReportSuccess("Insert 1300 rows")

	if err := tests.TestInsert1301(); err != nil {
		tests.ReportFailure("Insert 1301 rows", err)
		return
	}
	tests.ReportSuccess("Insert 1301 rows")

	if err := tests.TestInsertMaxStrings(); err != nil {
		tests.ReportFailure("Insert Max Strings", err)
		return
	}
	tests.ReportSuccess("Insert Max Strings")

	if err := tests.TestInsertTooLongStrings(); err != nil {
		tests.ReportSuccess("Insert Max Strings")
	} else {
		tests.ReportFailure("Insert Max Strings", err)
	}

	if err := tests.TestSelect(); err != nil {
		tests.ReportFailure("Select", err)
		return
	}
	tests.ReportSuccess("Select")
}
