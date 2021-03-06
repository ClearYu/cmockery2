/*
 * Copyright 2008 Google Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <inttypes.h>
#include <cmockery.h>
#include <database.h>

extern DatabaseConnection* connect_to_customer_database();
extern unsigned int get_customer_id_by_name(
    DatabaseConnection * const connection, const char * const customer_name);

// Mock query database function.
static unsigned int mock_query_database(
        DatabaseConnection* const connection, const char * const query_string,
        void *** const results) {
    *results = (void**)((uintptr_t)mock());
    return (unsigned int)mock();
}

// Mock of the connect to database function.
DatabaseConnection* connect_to_database(const char * const database_url,
                                        const unsigned int port) {
    return (DatabaseConnection*)((uintptr_t)mock());
}

static void test_connect_to_customer_database(void **state) {
    will_return(connect_to_database, 0x0DA7ABA53);
    assert_int_equal((uintptr_t)connect_to_customer_database(), 0x0DA7ABA53);
}

/* This test fails as the mock function connect_to_database() will have no
 * value to return. */
static void test_fail_connect_to_customer_database(void **state) {
    assert_true(connect_to_customer_database() ==
                (DatabaseConnection*)0x0DA7ABA53);
}

static void test_get_customer_id_by_name(void **state) {
    DatabaseConnection connection = {
        "somedatabase.somewhere.com", 12345678, mock_query_database
    };
    // Return a single customer ID when mock_query_database() is called.
    int customer_ids = 543;
    will_return(mock_query_database, &customer_ids);
    will_return(mock_query_database, 1);
    assert_int_equal(get_customer_id_by_name(&connection, "john doe"), 543);
}

int main(void) {
    const UnitTest tests[] = {
        unit_test(test_connect_to_customer_database),
        unit_test(test_get_customer_id_by_name),
        unit_test_expect_failure(test_fail_connect_to_customer_database),
    };
    return run_tests(tests, "customer_database");
}
