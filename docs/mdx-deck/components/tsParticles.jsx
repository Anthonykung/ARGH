import React from "react";
import Particles from "react-particles-js";

export default function App() {
  return (
    <Particles
      params={{
        background: {
          color: {
            value: "#000"
          }
        },
        particles: {
          number: {
            value: 160,
            density: {
              enable: false
            }
          },
          size: {
            value: 10,
            random: true
          },
          move: {
            direction: "bottom",
            outMode: "out"
          },
          links: {
            enable: false
          }
        },
        interactivity: {
          events: {
            onClick: {
              enable: true,
              mode: "remove"
            }
          },
          modes: {
            remove: {
              quantity: 10
            }
          }
        }
      }}
    />
  );
}
