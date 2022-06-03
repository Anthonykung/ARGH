import React from "react";
import Tooltip from '@mui/material/Tooltip';

export default function Code(props) {

  React.useEffect(() => {
    // Find cs-layout class and set height to 85vh
    const layout = document.getElementsByClassName('cs-layout')[0];
    if (layout) {
      layout.style.height = '85vh';
    }
    // Find cs-col-layout class and set height to 85vh
    const colLayout = document.getElementsByClassName('cs-col-layout')[0];
    if (colLayout) {
      colLayout.style.height = '85vh';
    }
  });

  return (
    <div style={{
      borderRadius: '0.5rem',
      backgroundColor: '#1d1f21',
      width: '90vw',
      height: '90vh',
    }}>
      <div style={{
        backgroundColor: '#1d1f21',
        borderRadius: '0.5rem 0.5rem 0 0',
        height: '40px',
        display: 'flex',
        alignItems: 'center',
        justifyContent: 'flex-start',
      }}>
      <Tooltip title="Don't click me >:[" placement="top">
        <span style={{
          height: '15px',
          width: '15px',
          borderRadius: '50%',
          backgroundColor: '#ff3e58',
          display: 'inline-block',
          margin: '0.25rem',
        }}>
        </span>
      </Tooltip>
        <Tooltip title="Don't click me >:[" placement="top">
          <span style={{
            height: '15px',
            width: '15px',
            borderRadius: '50%',
            backgroundColor: '#ffbf00',
            display: 'inline-block',
            margin: '0.25rem',
          }}>
          </span>
        </Tooltip>
        <Tooltip title="Don't click me >:[" placement="top">
          <span style={{
            height: '15px',
            width: '15px',
            borderRadius: '50%',
            backgroundColor: '#00db43',
            display: 'inline-block',
            margin: '0.25rem',
          }}>
          </span>
        </Tooltip>
        <Tooltip title="Don't click me >:[" placement="top">
          <span style={{
            height: '25px',
            width: '80%',
            display: 'inline-block',
            margin: '0.25rem',
            textAlign: 'left',
            color: 'rgba(255, 255, 255, 0.282)',
            fontSize: '1rem',
            padding: '0 0.5rem',
          }}>
            {props.type}
            <svg viewBox="0 0 30 30" style={{
              width: '10px',
              display: 'inline-block',
              fill: 'rgba(255, 255, 255, 0.282)',
              marginLeft: '4px',
            }}><polygon points="15,17.4 4.8,7 2,9.8 15,23 28,9.8 25.2,7 "></polygon></svg>
          </span>
        </Tooltip>
      </div>
      <div style={{
        padding: '0.5rem',
        backgroundColor: '#1d1f21',
        borderRadius: '0 0 0.5rem 0.5rem',
        color: 'white',
      }}>
        {props.children}
      </div>
    </div>
  );
}