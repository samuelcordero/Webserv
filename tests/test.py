import pytest
import requests

# Base URL for the web server
BASE_URL = "http://localhost:8080"

# checks for 200 status code in the root page
def test_root_page_status():
    response = requests.get(BASE_URL)
    assert response.status_code == 200

# Check GET request
def test_get_request():
    response = requests.get(BASE_URL + "/file")
    assert response.status_code == 200

# Check POST request
def test_post_request():
    response = requests.post(BASE_URL + "/file", data={"key": "value"})
    assert response.status_code == 200 

# Check for HEAD method
def test_head_request():
    response = requests.head(BASE_URL + "/file")
    assert response.status_code == 200
    assert response.content == b''  # No content in the response

# Check handling of an unknown request method
def test_unknown_request_method():
    response = requests.request("UNKNOWN", BASE_URL + "/file")
    assert response.status_code == 400  # Invalid request

# Check response to a wrong URL
def test_wrong_url():
    response = requests.get(BASE_URL + "/non_existent_resource")
    assert response.status_code == 404

# Check upload and download of a file
def test_file_upload_and_download():
    file_path = 'Makefile'
    file_name = 'file'
    upload_url = f"{BASE_URL}/{file_name}"

    # Read the file as binary data
    with open(file_path, 'rb') as f:
        file_data = f.read()

    # Send the binary data in the body of the POST request
    response = requests.post(upload_url, data=file_data)
    assert response.status_code == 200

    # Download the file to verify it was uploaded correctly
    response = requests.get(upload_url)
    assert response.status_code == 200
    with open(file_path, 'rb') as f:
        assert response.content == f.read()

# Check DELETE request
def test_delete_request():
    response = requests.delete(BASE_URL + "/file")
    assert response.status_code == 204 # No Content
