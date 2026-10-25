# Loudest Keyboard
## Description
This is meant to be an ncurses-based typing test, with very loud keys.

The file [words.txt](dict/words.txt) is an unmodified copy of Michael Wehar's [5000 common words](https://github.com/MichaelWehar/Public-Domain-Word-Lists/blob/master/5000-more-common.txt).

## Build from source
```
git clone https://github.com/charlesjchai/loudest-keyboard
cd loudest-keyboard
cmake -B build
cmake --build build
./build/keyboard
```

## License
This project is licensed under the GNU AGPL v3 for the code, and CC-BY-SA 4.0 for the audio files. See [LICENSE](LICENSE) and [AUDIO-LICENSE](audio/AUDIO-LICENSE) for more details.

## Acknowledgements
- ncurses
- Michael Wehar's [5000 common words](https://github.com/MichaelWehar/Public-Domain-Word-Lists/blob/master/5000-more-common.txt)