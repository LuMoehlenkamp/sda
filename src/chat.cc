#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <iostream>

using boost::asio::ip::tcp;

class AsyncReader {
public:
  AsyncReader(boost::asio::io_context &io_context, tcp::socket &socket)
      : io_context_(io_context), socket_(socket) {}

  void startAsyncRead() {
    boost::asio::async_read(
        socket_, boost::asio::buffer(data_),
        boost::bind(&AsyncReader::handleRead1, this,
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred));

    boost::asio::async_read(
        socket_, boost::asio::buffer(data_),
        boost::bind(&AsyncReader::handleRead2, this,
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred));

    io_context_.run(); // Start the asynchronous operations
  }

private:
  void handleRead1(const boost::system::error_code &error,
                   std::size_t bytes_transferred) {
    if (!error) {
      std::cout << "Handler 1 received: " << data_ << std::endl;
    } else {
      std::cout << "Error in handler 1: " << error.message() << std::endl;
    }
  }

  void handleRead2(const boost::system::error_code &error,
                   std::size_t bytes_transferred) {
    if (!error) {
      std::cout << "Handler 2 received: " << data_ << std::endl;
    } else {
      std::cout << "Error in handler 2: " << error.message() << std::endl;
    }
  }

  boost::asio::io_context &io_context_;
  tcp::socket &socket_;
  std::string data_;
};

int main() {
  boost::asio::io_context io_context;
  tcp::socket socket(io_context);

  // Assume socket connection setup here...

  AsyncReader async_reader(io_context, socket);
  async_reader.startAsyncRead();

  return 0;
}
