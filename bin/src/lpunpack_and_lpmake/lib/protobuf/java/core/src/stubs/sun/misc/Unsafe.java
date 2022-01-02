/*
 * Copyright (c) 2000, 2013, Oracle and/or its affiliates. All rights reserved.
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * This code is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 only, as
 * published by the Free Software Foundation.  Oracle designates this
 * particular file as subject to the "Classpath" exception as provided
 * by Oracle in the LICENSE file that accompanied this code.
 *
 * This code is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * version 2 for more details (a copy is included in the LICENSE file that
 * accompanied this code).
 *
 * You should have received a copy of the GNU General Public License version
 * 2 along with this work; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Please contact Oracle, 500 Oracle Parkway, Redwood Shores, CA 94065 USA
 * or visit www.oracle.com if you need additional information or have any
 * questions.
 */

package sun.misc;

/**
 * A stub class for sun.misc.Unsafe to make it possible to build protobuf only using Android SDK,
 * which lacks this class.
 *
 * Note that this class is only to provide symbol definitions at build-time and this class is not
 * embedded in the protobuf library. At runtime, Unsafe class is provided by the libcore-oj which is
 * in the bootclasspath and protobuf is linked to the class loaded there.
 */
public class Unsafe {
  public Object allocateInstance(Class<?> clazz) throws InstantiationException { /* null implementation */ return null; }
  public int arrayIndexScale(Class arrayClass) { /* null implementation */ return 0; }
  public int arrayBaseOffset(Class arrayClass) { /* null implementation */ return 0; }
  public void copyMemory(long srcAddress, long targetAddress, long length) { /* null implmentation */ }
  public void copyMemory(byte[] src, long srcIndex, byte[] target, long targetIndex, long length) { /* null implmentation */ }
  public boolean getBoolean(Object o, long offset) { /* null implementation */ return false; }
  public boolean getBoolean(long address) { /* null implementation */ return false; }
  public byte getByte(Object o, long offset) { /* null implementation */ return 0; }
  public byte getByte(long address) { /* null implementation */ return 0; }
  public float getFloat(Object o, long offset) { /* null implementation */ return 0; }
  public double getDouble(Object o, long offset) { /* null implementation */ return 0; }
  public int getInt(Object o, long offset) { /* null implementation */ return 0; }
  public int getInt(long address) { /* null implementation */ return 0; }
  public long getLong(Object o, long offset) { /* null implementation */ return 0; }
  public long getLong(long address) { /* null implementation */ return 0; }
  public Object getObject(Object o, long offset) { /* null implementation */ return null; }
  public long objectFieldOffset(java.lang.reflect.Field f) { /* null implementation */ return 0; }
  public void putBoolean(Object o, long offset, boolean x) { /* null implementation */ }
  public void putBoolean(long address, boolean x) { /* null implmentation */ }
  public void putByte(Object o, long offset, byte x) { /* null implementation */ }
  public void putByte(long address, byte x) { /* null implmentation */ }
  public void putFloat(Object o, long offset, float x) { /* null implementation */ }
  public void putDouble(Object o, long offset, double x) { /* null implementation */ }
  public void putInt(Object o, long offset, int x) { /* null implementation */ }
  public void putInt(long address, int x) { /* null implmentation */ }
  public void putLong(Object o, long offset, long x) { /* null implementation */ }
  public void putLong(long address, long x) { /* null implmentation */ }
  public void putObject(Object o, long offset, Object x) { /* null implementation */ }
  public Object staticFieldBase(java.lang.reflect.Field f) { /* null implementation */ return null; }
  public long staticFieldOffset(java.lang.reflect.Field f) { /* null implementation */ return 0; }
}
