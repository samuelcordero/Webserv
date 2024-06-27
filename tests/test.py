import pytest
import requests

# checks for Hello World! in the root page
def test_root_page_content():
    response = requests.get("http://localhost:8080")
    assert response.status_code == 200
    assert "Hello World!" in response.text

