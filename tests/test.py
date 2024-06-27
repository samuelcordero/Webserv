import pytest
import requests

# Base URL for the web server
BASE_URL = "http://localhost:8080"

# checks for Hello World! in the root page
def test_root_page_content():
    response = requests.get(BASE_URL)
    assert "Hello World!" in response.text

# checks for 200 status code in the root page
def test_root_page_status():
    response = requests.get(BASE_URL)
    assert response.status_code == 200

# Check GET request
def test_get_request():
    response = requests.get(BASE_URL + "/some_resource")
    assert response.status_code == 200

# Check POST request
def test_post_request():
    response = requests.post(BASE_URL + "/some_resource", data={"key": "value"})
    assert response.status_code == 201 # Created

# Check DELETE request
def test_delete_request():
    response = requests.delete(BASE_URL + "/some_resource")
    assert response.status_code == 204 # No Content

# Check handling of an unknown request method
def test_unknown_request_method():
    response = requests.request("UNKNOWN", BASE_URL + "/some_resource")
    assert response.status_code == 405  # Method Not Allowed

# Check response to a wrong URL
def test_wrong_url():
    response = requests.get(BASE_URL + "/non_existent_resource")
    assert response.status_code == 404