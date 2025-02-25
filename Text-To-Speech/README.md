## Usage
cd Text-To-Speech/ \
echo 'This sentence is spoken first. This sentence is synthesized while the first sentence is spoken.' | \
  ./piper/piper --model en_US-lessac-medium.onnx --output-raw | \
  aplay -r 22050 -f S16_LE -t raw -
