class Iir < Formula
  desc "IIR realtime filter library written in C++"
  url "https://github.com/berndporr/iir1/archive/1.9.0.zip"
  homepage "https://github.com/berndporr/iir1"
  depends_on "cmake" => :build
  sha256 "eb2512838ced9aa100e279dca96cc0a84e47672e57ec23c67ca56b18848c27ed"

  def install
    system "cmake", ".", *std_cmake_args
    system "make"
    system "make", "install"
  end

  test do
    system "ctest"
  end
end
