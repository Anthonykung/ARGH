import React from "react";
import Tooltip from '@mui/material/Tooltip';

export default function Browser(props) {
  return (
    <div style={{
      borderRadius: '0.5rem',
      backgroundColor: '#f5f5f5',
      width: '80%',
    }}>
      <div style={{
        backgroundColor: '#e0e0e0',
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
            borderRadius: '0.5rem',
            backgroundColor: 'white',
            display: 'inline-block',
            margin: 'auto',
            textAlign: 'left',
            color: '#333',
            fontSize: '1rem',
            padding: '0 0.5rem',
          }}>
            {props.title}
          </span>
        </Tooltip>
      </div>
      <div style={{
        padding: '0.5rem',
        backgroundColor: 'white',
        borderRadius: '0 0 0.5rem 0.5rem',
        color: '#333',
      }}>
        {props.children}
      </div>
    </div>
  );
}