/*
 * Demo for the custom native <ColorBox> component.
 *
 * Exercises everything a custom Fabric component needs:
 *   - PROPS (string):  three boxes with different `color` props
 *   - PROPS (number):  the wide box's `opacity` animates over time, proving prop
 *                      UPDATES (not just initial values) reach the native side
 *   - LAYOUT:          flexbox (a row of equal cells + a full-width box) drives
 *                      each native view's size via ILayoutDelegate::updateLayout
 */
import React, {useEffect, useState} from 'react';
import {StyleSheet, Text, View} from 'react-native';
import ColorBox from './ColorBox';

export const App = () => {
  const [tick, setTick] = useState(0);
  useEffect(() => {
    const id = setInterval(() => setTick(v => v + 1), 700);
    return () => clearInterval(id);
  }, []);
  const pulsing = 0.35 + 0.65 * Math.abs(Math.sin(tick * 0.5));

  return (
    <View style={styles.root}>
      <Text style={styles.title}>Custom native &lt;ColorBox&gt; · Amazon Vega IView</Text>
      <Text style={styles.sub}>
        props: color (string) + opacity (number) · layout: flexbox
      </Text>

      {/* color prop (string) + flex layout: three equal cells in a row */}
      <View style={styles.row}>
        <ColorBox style={styles.cell} color="#ef4444" />
        <ColorBox style={styles.cell} color="#22c55e" />
        <ColorBox style={styles.cell} color="#3b82f6" />
      </View>

      {/* opacity prop (number) animates -> proves prop updates reach native */}
      <ColorBox style={styles.wide} color="#f59e0b" opacity={pulsing} />
    </View>
  );
};

const styles = StyleSheet.create({
  root: {flex: 1, backgroundColor: '#0b0b0f', padding: 32},
  title: {color: '#ffffff', fontSize: 30, fontWeight: '700'},
  sub: {color: '#9aa0aa', fontSize: 18, marginTop: 6, marginBottom: 28},
  row: {flexDirection: 'row', height: 220},
  cell: {flex: 1, marginRight: 16},
  wide: {height: 160, marginTop: 28},
});
