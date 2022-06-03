import { themes } from 'mdx-deck';

// Example theme.js
export default {
  ...themes.comic,
  ...themes.prism,
  colors: {
    text: '#adbac7',
    background: '#22272e',
    primary: '#539bf5',
  },
  styles: {
    Slide: {
    },
    Headers: {

    },
    Footer: {

    },
    p: {
      padding: '2%',
    },
  },
}